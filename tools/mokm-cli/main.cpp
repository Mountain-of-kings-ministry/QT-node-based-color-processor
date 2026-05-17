#include <iostream>
#include <string>
#include <vector>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QIODevice>

#include "mokm/ImageBuffer.h"
#include "mokm/NodeGraph.h"
#include "mokm/OCIOWrapper.h"
#include "mokm/PipelineExecutor.h"

static void printUsage(const char *name)
{
    std::cerr << "MOKM Color Processor - Headless CLI\n\n";
    std::cerr << "Usage:\n";
    std::cerr << "  " << name << " --graph <graph.json> --input <path> --output <path>\n";
    std::cerr << "  " << name << " --graph <graph.json> --sequence <pattern> --range <start-end>\n\n";
    std::cerr << "Options:\n";
    std::cerr << "  --graph <file>        Node graph JSON description\n";
    std::cerr << "  --input <file>        Input image file\n";
    std::cerr << "  --output <file>       Output image file\n";
    std::cerr << "  --sequence <pattern>  Image sequence pattern (e.g., frame.%04d.exr)\n";
    std::cerr << "  --range <start-end>   Frame range for sequence (e.g., 1-100)\n";
    std::cerr << "  --config <file>       OCIO config file (overrides graph config)\n";
    std::cerr << "  --list-cs             List available color spaces from config\n";
    std::cerr << "  --help                Show this help\n";
}

static int listColorSpaces(const std::string& configPath)
{
    auto& ocio = mokm::OCIOWrapper::instance();
    if (!ocio.loadConfig(configPath)) {
        std::cerr << "Failed to load config: " << configPath << "\n";
        return 1;
    }

    std::cout << "Color Spaces:\n";
    for (const auto& cs : ocio.colorSpaces())
        std::cout << "  " << cs << "\n";

    std::cout << "\nDisplays:\n";
    for (const auto& d : ocio.displays()) {
        std::cout << "  " << d << ":\n";
        for (const auto& v : ocio.views(d))
            std::cout << "    - " << v << "\n";
    }

    std::cout << "\nLooks:\n";
    for (const auto& l : ocio.looks())
        std::cout << "  " << l << "\n";

    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("MOKM Color Processor");
    QCoreApplication::setApplicationVersion("0.1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Node-based color processing tool");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption graphOpt("graph", "Path to node graph JSON", "file");
    QCommandLineOption inputOpt("input", "Input image file", "path");
    QCommandLineOption outputOpt("output", "Output image file", "path");
    QCommandLineOption seqOpt("sequence", "Image sequence pattern", "pattern");
    QCommandLineOption rangeOpt("range", "Frame range (start-end)", "range");
    QCommandLineOption configOpt("config", "OCIO config file", "file");
    QCommandLineOption listCSOpt("list-cs", "List color spaces from config");

    parser.addOption(graphOpt);
    parser.addOption(inputOpt);
    parser.addOption(outputOpt);
    parser.addOption(seqOpt);
    parser.addOption(rangeOpt);
    parser.addOption(configOpt);
    parser.addOption(listCSOpt);

    parser.process(app);

    QString configPath = parser.value(configOpt);

    if (parser.isSet(listCSOpt)) {
        if (configPath.isEmpty()) {
            std::cerr << "Error: --config is required with --list-cs\n";
            return 1;
        }
        return listColorSpaces(configPath.toStdString());
    }

    bool hasGraph = parser.isSet(graphOpt);
    bool hasInput = parser.isSet(inputOpt);
    bool hasOutput = parser.isSet(outputOpt);
    bool hasSequence = parser.isSet(seqOpt);
    bool hasRange = parser.isSet(rangeOpt);

    if (!hasGraph) {
        std::cerr << "Error: --graph is required\n";
        printUsage(argv[0]);
        return 1;
    }

    mokm::PipelineExecutor executor;
    if (hasGraph) {
        // Load graph from file or parse as JSON string
        QString graphVal = parser.value(graphOpt);
        QFile file(graphVal);
        if (file.exists()) {
            if (!file.open(QIODevice::ReadOnly)) {
                std::cerr << "Error: Cannot open graph file: " << graphVal.toStdString() << "\n";
                return 1;
            }
            std::string json = file.readAll().toStdString();
            if (!executor.loadGraphFromJson(json)) {
                std::cerr << "Error: Failed to load graph: " << executor.lastError() << "\n";
                return 1;
            }
        } else {
            // Treat as inline JSON
            if (!executor.loadGraphFromJson(graphVal.toStdString())) {
                std::cerr << "Error: Failed to parse graph JSON: " << executor.lastError() << "\n";
                return 1;
            }
        }
    }

    // Override config if provided
    if (!configPath.isEmpty()) {
        auto& ocio = mokm::OCIOWrapper::instance();
        if (!ocio.loadConfig(configPath.toStdString())) {
            std::cerr << "Error: Failed to load OCIO config: " << configPath.toStdString() << "\n";
            return 1;
        }
    }

    if (hasInput && hasOutput) {
        mokm::ImageBuffer buf;
        if (!buf.loadFromFile(parser.value(inputOpt).toStdString())) {
            std::cerr << "Error: Failed to load input image\n";
            return 1;
        }

        if (buf.colorSpace().empty())
            buf.setColorSpace("scene_linear");

        if (!executor.processImage(buf)) {
            std::cerr << "Error: Processing failed: " << executor.lastError() << "\n";
            return 1;
        }

        if (!buf.saveToFile(parser.value(outputOpt).toStdString())) {
            std::cerr << "Error: Failed to save output image\n";
            return 1;
        }

        std::cout << "Processed: " << parser.value(inputOpt).toStdString()
                  << " -> " << parser.value(outputOpt).toStdString() << "\n";

    } else if (hasSequence && hasRange) {
        QString rangeStr = parser.value(rangeOpt);
        int dashPos = rangeStr.indexOf('-');
        if (dashPos < 0) {
            std::cerr << "Error: Invalid range format. Use start-end (e.g., 1-100)\n";
            return 1;
        }
        int start = rangeStr.left(dashPos).toInt();
        int end = rangeStr.mid(dashPos + 1).toInt();

        if (!executor.processSequence(
                parser.value(seqOpt).toStdString(), start, end,
                parser.value(outputOpt).toStdString())) {
            std::cerr << "Error: Sequence processing failed\n";
            return 1;
        }

        std::cout << "Processed frames " << start << "-" << end << "\n";
    } else {
        std::cerr << "Error: Specify --input/--output or --sequence/--range\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
